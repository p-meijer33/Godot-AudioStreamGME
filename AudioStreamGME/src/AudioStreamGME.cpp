/* godot-cpp integration testing project.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "AudioStreamGME.h"
#include "gme/gme.h"
#include "godot_cpp/core/error_macros.hpp"

#include <cstdint>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/multiplayer_peer.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/audio_frame.hpp>

using namespace godot;


AudioStreamGME::AudioStreamGME()
        : mix_rate(44100), stereo(true), hz(639) {
        AudioStreamGME::emu_=nullptr;
}

AudioStreamGME::~AudioStreamGME(){
    gme_delete(emu_);
}

void AudioStreamGME::load_file(const String& path){
    if (emu_) {
        gme_delete(emu_);
    }
    gme_open_file(path.utf8().get_data(), &emu_, SAMPLE_RATE);
    if (!emu_)
	{
		//char msg[35] = "Failed to create GME emulator for ";
		//strcat(msg, path.utf8().get_data());
		UtilityFunctions::print("Failed to create GME emulator");
		UtilityFunctions::print(path.utf8().get_data());
		return;
	}
	UtilityFunctions::print("Succeeded to create GME emulator");
	if (gme_load_file(emu_, path.utf8().get_data()) != 0 ){
		UtilityFunctions::print("Failed to load NSF File");
		gme_delete(emu_);
		emu_ = nullptr;
		return;
	}
	//gme_start_track(emu_,0);
}

void AudioStreamGME::load_data(const PackedByteArray &p_data){
    if (emu_) {
        gme_delete(emu_);
    }
    const uint8_t *bytes = p_data.ptr();
    const int64_t size = p_data.size();

    gme_open_data(bytes, static_cast<long>(size), &emu_, SAMPLE_RATE );

    if (!emu_)
	{
		//char msg[35] = "Failed to create GME emulator for ";
		//strcat(msg, path.utf8().get_data());
		UtilityFunctions::print("Failed to create GME emulator");
		return;
	}
	UtilityFunctions::print("Succeeded to create GME emulator");
	//if (gme_load_data(emu_, bytes, static_cast<long>(size)) ){
	//	UtilityFunctions::print("Failed to load NSF Data");
	//	gme_delete(emu_);
	//	emu_ = nullptr;
	//	return;
	//}
}

int AudioStreamGME::get_track_count(){
    return gme_track_count(emu_);
}

void AudioStreamGME::start_track(uint16_t track){
    gme_start_track(emu_,track);
}


Ref<AudioStreamPlayback> AudioStreamGME::instance_playback() {
    Ref<AudioStreamPlaybackGME> playback;
    playback.instantiate();
    playback->base = Ref<AudioStreamGME>(this);
    return playback;
}

Ref<AudioStreamPlayback>AudioStreamGME:: _instantiate_playback() const {
    Ref<AudioStreamPlaybackGME> playback;
    playback.instantiate();
    playback->base = Ref<AudioStreamGME>(this);
    return playback;
}

String AudioStreamGME::get_stream_name() const {
    return "AudioStreamGME";
}
void AudioStreamGME::reset() {
    //set_position(0);
    gme_set_tempo(emu_, 0);
    gme_seek(emu_, 0);
}
void AudioStreamGME::set_position(uint64_t p) {
    gme_seek(emu_, p);
}

void AudioStreamGME::play_music(int16_t *pcm_buf, int size){
    gme_play(emu_,size, pcm_buf);
}

void AudioStreamGME::set_tempo(float tempo){
    gme_set_tempo(emu_, tempo);
}

void AudioStreamGME::pause(){
    gme_set_tempo(emu_, 0);
    gme_mute_voices(emu_, -1);
}

void AudioStreamGME::unpause(){
    gme_set_tempo(emu_, 1.0);
    gme_mute_voices(emu_, 0);
}



void AudioStreamGME::_bind_methods() {
    ClassDB::bind_method(D_METHOD("reset"), &AudioStreamGME::reset);
    ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamGME::get_stream_name);
    ClassDB::bind_method(D_METHOD("load_file"), &AudioStreamGME::load_file);
    ClassDB::bind_method(D_METHOD("load_data"), &AudioStreamGME::load_data);
    ClassDB::bind_method(D_METHOD("start_track"), &AudioStreamGME::start_track);
    ClassDB::bind_method(D_METHOD("get_track_count"), &AudioStreamGME::get_track_count);
    ClassDB::bind_method(D_METHOD("pause"), &AudioStreamGME::pause);
    ClassDB::bind_method(D_METHOD("unpause"), &AudioStreamGME::unpause);
    ClassDB::bind_method(D_METHOD("set_tempo"), &AudioStreamGME::set_tempo);
   
}

#define zeromem(to, count) memset(to, 0, count)

AudioStreamPlaybackGME::AudioStreamPlaybackGME() {
    active=false;
    
    AudioServer::get_singleton()->lock();
    //pcm_buffer = AudioServer::get_singleton()->audio_data_alloc(PCM_BUFFER_SIZE);
    pcm_buffer = memalloc(PCM_BUFFER_SIZE);
    zeromem(pcm_buffer, PCM_BUFFER_SIZE);
    AudioServer::get_singleton()->unlock();
    
}
AudioStreamPlaybackGME::~AudioStreamPlaybackGME() {
        if(pcm_buffer) {
        memfree(pcm_buffer);
        pcm_buffer = NULL;
    }
    
}
void AudioStreamPlaybackGME::stop() {
    active = false;
    base->reset();
}
void AudioStreamPlaybackGME::_start(float p_from_pos) {
    seek(p_from_pos);
    active = true;
}
void AudioStreamPlaybackGME::seek(float p_time) {
    float max = get_length();
    if (p_time < 0) {
            p_time = 0;
    }
    base->set_position(uint64_t(p_time * base->SAMPLE_RATE) << MIX_FRAC_BITS);
}



int32_t AudioStreamPlaybackGME::_mix(AudioFrame *p_buffer, float p_rate, int p_frames) {
    ERR_FAIL_COND_V(!active, 0);
    if (!active) {
        return 0;
    }

    zeromem(pcm_buffer,PCM_BUFFER_SIZE);
    int16_t *buf = (int16_t *)pcm_buffer;

    base->play_music(buf, p_frames * 2);
    for(int i = 0; i < p_frames; i++) {
        float left = float(buf[i * 2]) / 32767.0;
        float right = float(buf[i * 2 + 1]) / 32767.0;
        p_buffer[i] = {left,right};
    }

    return p_frames;
  
}

int AudioStreamPlaybackGME::get_loop_count() const {
    return 0;
}
float AudioStreamPlaybackGME::get_playback_position() const {
    return 0.0;
}
float AudioStreamPlaybackGME::get_length() const {
    return 0.0;
}
bool AudioStreamPlaybackGME::is_playing() const {
    return active;
}


void AudioStreamPlaybackGME::_bind_methods() {
    // Required by GDCLASS macro
}

