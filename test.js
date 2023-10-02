const fs = require('fs-extra');
const { SerialPort } = require('serialport');
const Mpv = require('node-mpv');

const port = "/dev/cu.usbmodem1101";
const bRate = 38400;

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

// play video with MPV
function playVideo() {
    const mpv = new Mpv({
        'audio_only': false,
        'fullscreen': true,
        'loop': true,
        'volume': 100
    });

    // restart mpv when stopped
    mpv.on('stopped', () => {
        mpv.load('video.mp4');
        mpv.play();
    });

    // start the video
    mpv.load('video.mp4');
    // play the video
    mpv.play();

    return mpv;
}

// serial communication with Arduino
const arduinoPort = new SerialPort({ path: port, baudRate: bRate }, (error) => {
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
        console.log('Timestamps:', timestamps);
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

// process timestamps based on the current time of the video
function processTimestamps(timestamps, videoPlayer) {
    const alreadyProcessed = new Set();

    const interval = setInterval(async () => {
        try {
            const currentTime = await videoPlayer.getProperty('time-pos');
            console.log('Current Video Time:', currentTime);

            for (const timestamp of timestamps) {
                if (
                    !alreadyProcessed.has(timestamp.time) &&
                    currentTime >= timestamp.time
                ) {
                    const arduinoData = `${timestamp.selectedOption},${timestamp.integerValue}\n`;
                    arduinoPort.write(arduinoData);
                    console.log('Timestamp:', timestamp.time, ' Sent to Arduino:', arduinoData);

                    alreadyProcessed.add(timestamp.time);
                }
            }
        } catch (error) {
            console.error('Error getting video time:', error.message);
        }
    }, 100); // check every...

    videoPlayer.on('stopped', () => {
        clearInterval(interval);
        console.log('Video playback has ended.');
    });
    videoPlayer.on('start', () => {
        console.log('Video player started.');
    });
    
    videoPlayer.on('play', () => {
        console.log('Video player is playing.');
    });
    
}

startVideoAndTimestamps();
