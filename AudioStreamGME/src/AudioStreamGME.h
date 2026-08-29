/* godot-cpp integration testing project.
 *
 * This is free and unencumbered software released into the public domain.
 */

#pragma once

// We don't need windows.h in this example plugin but many others do, and it can
// lead to annoying situations due to the ton of macros it defines.
// So we include it and make sure CI warns us if we use something that conflicts
// with a Windows define.
#include <cstdint>
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/variant/string.hpp>

//#include <godot_cpp/classes/control.hpp>
//#include <godot_cpp/classes/global_constants.hpp>
//#include <godot_cpp/classes/image.hpp>
//#include <godot_cpp/classes/input_event_key.hpp>
//#include <godot_cpp/classes/tile_map.hpp>
//#include <godot_cpp/classes/tile_set.hpp>
//#include <godot_cpp/classes/tween.hpp>
//#include <godot_cpp/classes/viewport.hpp>
//#include <godot_cpp/variant/variant.hpp>
//#include <godot_cpp/variant/variant_internal.hpp>
//#include <godot_cpp/classes/audio_stream.hpp>
//#include <godot_cpp/classes/audio_stream_playback.hpp>

//#if GODOT_VERSION_MINOR >= 4
//#include <godot_cpp/variant/typed_dictionary.hpp>
//#endif // GODOT_VERSION_MINOR >= 4

//#include <godot_cpp/core/binder_common.hpp>
//#include <godot_cpp/core/gdvirtual.gen.inc>

#include <gme/gme.h>

using namespace godot;

class AudioStreamGME : public AudioStream {
    GDCLASS(AudioStreamGME, AudioStream)

private:
    friend class AudioStreamPlaybackGME;
    uint64_t pos;
    int mix_rate;
    bool stereo;
    int hz;

    Music_Emu* emu_;

    enum {
        SAMPLE_RATE = 44100
    };

public:
    void reset();
    void set_position(uint64_t pos);
    virtual Ref<AudioStreamPlayback> instance_playback();
    Ref<AudioStreamPlayback> _instantiate_playback() const override;
    virtual String get_stream_name() const;
    //void gen_tone(int16_t *pcm_buf, int size);
    void play_music(int16_t *pcm_buf, int size);
    virtual float get_length() const { return 0; } // if supported, otherwise return 0
    void load_file(const String& path);
    void load_data(const PackedByteArray &p_data);
    int get_track_count();
    void start_track(uint16_t track);
    void pause();
    void unpause();
    void set_tempo(float tempo);

    AudioStreamGME();
    ~AudioStreamGME();

protected:
    static void _bind_methods();
};


class AudioStreamPlaybackGME : public AudioStreamPlayback {
    GDCLASS(AudioStreamPlaybackGME, AudioStreamPlayback)
    friend class AudioStreamGME;

private:
    enum {
        PCM_BUFFER_SIZE = 8192
    };
    enum {
        MIX_FRAC_BITS = 13,
        MIX_FRAC_LEN = (1 << MIX_FRAC_BITS),
        MIX_FRAC_MASK = MIX_FRAC_LEN - 1,
    };
    void *pcm_buffer;
    Ref<AudioStreamGME> base;
    bool active;

public:
    virtual void _start(float p_from_pos = 0.0);
    virtual void stop();
    virtual bool is_playing() const;
    virtual int get_loop_count() const; // times it looped
    virtual float get_playback_position() const;
    virtual void seek(float p_time);
    virtual int32_t _mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) override;
    virtual float get_length() const; // if supported, otherwise return 0
    AudioStreamPlaybackGME();
    ~AudioStreamPlaybackGME();
protected:
    static void _bind_methods();
};


