const { spawn } = require('child_process');
// const request = require('request');
const VLC = require("vlc-client");

const videoPlayer = spawn('/Applications/VLC.app/Contents/MacOS/VLC', [
    'video.mp4',
    '--fullscreen',
    '--video-on-top',
    '--loop',
]);

let vlc;

setTimeout(() => {
    vlc = new VLC.Client({
        ip: "localhost",
        port: 8080,
        username: "", //username is optional
        password: "password"
    });

    setTimeAsync();
}, 3000);


async function setTimeAsync() {
    try {
        await vlc.setTime(90);
        console.log('Time set successfully');
    } catch (error) {
        console.error('Error:', error);
    } finally {
    }
}
