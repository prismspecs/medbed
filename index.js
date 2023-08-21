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
  // const videoPlayer = spawn('vlc', ['video.mp4', '--fullscreen']);
  const videoPlayer = spawn('vlc', ['video.mp4']);
  videoPlayer.on('error', (err) => {
    console.error('Video player error:', err.message);
  });
  return videoPlayer;
}

// serial communication with Arduino
const arduinoPort = new SerialPort({ path: '/dev/ttyACM0', baudRate: 9600 });

// process timestamps based on the current time of the video
async function processTimestamps(timestamps, videoPlayer) {
  const startTime = new Date();
  const alreadyProcessed = new Set();

  const interval = setInterval(() => {
    const currentTime = new Date() - startTime;
    for (const timestamp of timestamps) {
      if (
        !alreadyProcessed.has(timestamp.time) &&
        currentTime >= timestamp.time * 1000
      ) {
        console.log(
          `Time: ${timestamp.time}s, Option: ${timestamp.selectedOption}, Value: ${timestamp.integerValue}`
        );

        // send data to Arduino
        const arduinoData = `${timestamp.selectedOption},${timestamp.integerValue}\n`;
        arduinoPort.write(arduinoData);

        alreadyProcessed.add(timestamp.time);
      }
    }
  }, 1000); // Check every second

  videoPlayer.on('exit', () => {
    clearInterval(interval);
    console.log('Video playback has ended.');
  });
}

async function main() {
  const timestamps = await readTimestamps();
  if (timestamps.length === 0) {
    console.error('No timestamps found.');
    return;
  }

  const videoPlayer = playVideo();

  arduinoPort.on('open', () => {
    console.log('Serial communication with Arduino established.');
    processTimestamps(timestamps, videoPlayer);
  });

  arduinoPort.on('error', (err) => {
    console.error('Serial port error:', err.message);
  });
}

main();
