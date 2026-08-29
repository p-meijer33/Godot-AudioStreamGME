extends Node

@onready var audio_stream_player: AudioStreamPlayer = $AudioStreamPlayer
@onready var a_back_1: TextureButton = $ABack1

var track : int =0;
var maxtracks : int = 0;
var AS = AudioStreamGME.new();

func _ready() -> void:
	
	AS.load_file("Solstice - Die Suche Nach Dem Zauberstab Von Demnos.nsfe");
	#AS.load_file("test.nsf");
	AS.start_track(track); # do not start a track number higher than the number of tracks in the file, or it will crash
	
	maxtracks = AS.get_track_count()
	print("Number of tracks: ", maxtracks);
	audio_stream_player.stream = AS;
	audio_stream_player.play(0.0) # I get noise!  
	



func _on_a_back_1_pressed() -> void:
	#print("Button Pressed.");
	track = track - 1
	if (track < 0):
		track = maxtracks -1
	AS.start_track(track)
	pass # Replace with function body.




func _on_a_next_1_pressed() -> void:
	track = (track + 1) % maxtracks
	AS.start_track(track)
	pass # Replace with function body.


func _on_a_pause_1_pressed() -> void:
	AS.pause()
	pass # Replace with function body.


func _on_a_play_1_pressed() -> void:
	print("Play");
	AS.unpause()
	pass # Replace with function body.
