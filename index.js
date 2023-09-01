const fs = require('fs-extra');
const { spawn } = require('child_process');
const { SerialPort } = require('serialport');

// read timestamps from JSON file
async function readTimestamps() {
    try {
        const data = await fs.readFile('timestamps.json');
        return JSON.parse(data);
    } catch (error) {
        console.error('Error reading timestamps:', error.message);
        return [];
    }
}

// play video in full screen
function playVideo() {
    const videoPlayer = spawn('vlc', ['video.mp4', '--fullscreen', '--video-on-top', '--loop']);
    // const videoPlayer = spawn('vlc', ['video.mp4']);
    videoPlayer.on('error', (err) => {
        console.error('Video player error:', err.message);
    });

    videoPlayer.stdout.on('data', (data) => {

        // any messages from VLC?
        console.log(data.toString());

        // detect the video restart message and increment loop count
        if (data.toString().includes('playing again')) {
            videoLoopCount++;
            console.log(`Video looped through once. Loop count: ${videoLoopCount}`);
        }
    });

    return videoPlayer;
}

// serial communication with Arduino
const arduinoPort = new SerialPort({ path: '/dev/ttyACM0', baudRate: 9600 }, (error) => {
    if (error) {
        console.error('Error opening serial port:', error.message);
    } else {
        console.log('Serial communication with Arduino established.');
        startVideoAndTimestamps();
    }
});

async function startVideoAndTimestamps() {
    try {
        const timestamps = await readTimestamps();
        if (timestamps.length === 0) {
            console.error('No timestamps found.');
            return;
        }

        const videoPlayer = playVideo();

        arduinoPort.on('error', (err) => {
            console.error('Serial port error:', err.message);
        });

        processTimestamps(timestamps, videoPlayer);
    } catch (error) {
        console.error('Error:', error.message);
    }
}

// process timestamps based on the current time of the video
function processTimestamps(timestamps, videoPlayer) {
    const startTime = new Date();
    const alreadyProcessed = new Set();

    const interval = setInterval(() => {
        const currentTime = new Date() - startTime;
        for (const timestamp of timestamps) {
            if (
                !alreadyProcessed.has(timestamp.time) &&
                currentTime >= timestamp.time * 1000
            ) {
                const arduinoData = `${timestamp.selectedOption},${timestamp.integerValue}\n`;
                arduinoPort.write(arduinoData);
                console.log('Sent to Arduino:', arduinoData);

                alreadyProcessed.add(timestamp.time);
            }
        }
    }, 100); // check every...

    videoPlayer.on('exit', () => {
        clearInterval(interval);
        console.log('Video playback has ended.');
    });
}