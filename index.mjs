import mpv from 'mpv';
import fs from 'fs';
import SerialPort from 'serialport';

const videoFilePath = 'video.mp4';
const timestampsFilePath = 'timestamps.json';
const arduinoPort = '/dev/ttyACM0'; // Update this to your Arduino's port

// Function to read the timestamps from timestamps.json
function readTimestamps() {
  try {
    const jsonData = fs.readFileSync(timestampsFilePath, 'utf8');
    return JSON.parse(jsonData);
  } catch (error) {
    console.error('Error reading timestamps.json:', error.message);
    return [];
  }
}

// Function to find and output matching data based on the video's current time
function findMatchingData(timestamps, currentTime, matchedTimestamps, arduinoPort) {
  const matchingData = timestamps.find((item) => Math.abs(currentTime - item.time) < 2); // Allow a 2-second tolerance
  if (matchingData && !matchedTimestamps.has(matchingData.time)) {
    matchedTimestamps.add(matchingData.time);
    console.log('Matching data:', matchingData);
    // Send data to Arduino via Serial communication
    writeToArduino(arduinoPort, matchingData.selectedOption, matchingData.integerValue);
  }
}

// Function to write data to Arduino via Serial communication
function writeToArduino(port, selectedOption, integerValue) {
  const arduinoSerialPort = new SerialPort(port, { baudRate: 9600 });

  arduinoSerialPort.write(`${selectedOption},${integerValue}`, (err) => {
    if (err) {
      console.error('Error writing to Arduino:', err.message);
    } else {
      console.log('Data sent to Arduino:', selectedOption, integerValue);
    }
  });
}

// Main function to play the video and monitor progress
async function main() {
  const timestamps = readTimestamps();

  if (!timestamps.length) {
    console.error('No timestamps found or error reading timestamps.json. Exiting.');
    return;
  }

  const mpvPlayer = mpv(); // Create a new instance of the mpv player

  mpvPlayer.load(videoFilePath, { fullscreen: true });

  // Use a Set to store matched timestamps
  const matchedTimestamps = new Set();

  // Monitor the video's progress
  mpvPlayer.on('property-change', (property, value) => {
    if (property === 'time-pos') {
      const currentTime = parseFloat(value);
      findMatchingData(timestamps, currentTime, matchedTimestamps, arduinoPort);
    }
  });

  mpvPlayer.on('error', (error) => {
    console.error('mpv error:', error);
  });
}

main().catch((error) => {
  console.error('An error occurred:', error);
});
