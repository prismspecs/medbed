const VLC = require("vlc-client");
const { spawn } = require('child_process');

let vlc;

// Function to initialize VLC and return a promise that resolves when it's ready
function initializeVLC() {
    return new Promise((resolve, reject) => {
        vlc = new VLC.Client({
            ip: "localhost",
            port: 8080,
            username: "", // username is optional
            password: "password"
        });
    });
}

// Function to start VLC and initialize it
async function startVLC() {
    const videoPlayer = spawn('/Applications/VLC.app/Contents/MacOS/VLC', [
        'video.mp4',
        '--fullscreen',
        '--video-on-top',
        '--loop',
    ]);

    try {
        vlc = await initializeVLC();
        // Now you can perform actions with VLC using the 'vlc' object
        // For example, you can get the current video time as follows:
        const time = await vlc.getTime();
        console.log('Current video time:', time);
    } catch (err) {
        // Handle any errors here
        console.error('Failed to initialize VLC:', err);
    }
}

// Call the function to start VLC and initialize it
startVLC();
