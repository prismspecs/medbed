const mpvAPI = require('node-mpv');
const mpv = new mpvAPI();

// Start the player
mpv.start()
.then(() => {
	// This will load and start the song
	return mpv.load('/path/to/your/favorite/song.mp3')
})
.then(() => {
	// Set the volume to 50%
	return mpv.volume(50);
})
// this catches every arror from above
.catch((error) => {
	console.log(error);
});

// This will bind this function to the stopped event
mpv.on('stopped', () => {
    console.log("Your favorite song just finished, let's start it again!");
    mpv.loadFile('/path/to/your/favorite/song.mp3');
});