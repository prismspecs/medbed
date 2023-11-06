const { spawn } = require('child_process');
const { SerialPort } = require('serialport');
const fs = require('fs-extra');

const port = "/dev/cu.usbmodem1101";
const bRate = 38400;

// first serial communication with Arduino
const arduinoPort = new SerialPort({ path: port, baudRate: bRate }, (error) => {
    if (error) {
        console.error('Error opening serial port:', error.message);
    } else {
        console.log('Serial communication with Arduino established.');
        loadTimestamps();
    }
});

// then load timestamps
async function loadTimestamps() {
    try {
        const timestamps = await readTimestamps();
        if (timestamps.length === 0) {
            console.error('No timestamps found.');
            return;
        }

        // const videoPlayer = playVideo();

        arduinoPort.on('error', (err) => {
            console.error('Serial port error:', err.message);
        });

        processTimestamps(timestamps, videoPlayer);
    } catch (error) {
        console.error('Error:', error.message);
    }
}

// now start up VLC
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

    // setTimeAsync();
    // get current video time from VLC
    // vlc.getTime().then((time) => {
    //     console.log('Current video time:', time);
    // });
    
}, 3000);

// process timestamps based on the current time of the video
function processTimestamps(timestamps, videoPlayer) {
    const startTime = new Date();
    const alreadyProcessed = new Set();

    const interval = setInterval(() => {
        //const currentTime = new Date() - startTime;

        // get current video time

        vlc.getTime().then((time) => {
            console.log('Current video time:', time);
            const currentTime = time;

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
        });
    }, 50); // check every...

    videoPlayer.on('stopped', () => {
        clearInterval(interval);
        console.log('Video playback has ended.');
    });
}





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


async function setTimeAsync() {
    try {
        await vlc.setTime(90);
        console.log('Time set successfully');
    } catch (error) {
        console.error('Error:', error);
    } finally {
    }
}
