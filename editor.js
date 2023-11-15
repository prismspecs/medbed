const video = document.getElementById('video');
const timestampsContainer = document.getElementById('timestampsContainer');
const saveBtn = document.getElementById('saveBtn');
const loadFileInput = document.getElementById('loadFile');
const loadBtn = document.getElementById('loadBtn');
// quick edits
const btnAddTimestamps = document.getElementById('btnAddTimestamps');
const btnLEDsOff = document.getElementById('btnLEDsOff');
const btnLEDsBlue = document.getElementById('btnLEDsBlue');
const btnLEDsRed = document.getElementById('btnLEDsRed');
const btnLEDsWhite = document.getElementById('btnLEDsWhite');
const btnLEDsCycle = document.getElementById('btnLEDsCycle');
const btnRoomLights1Off = document.getElementById('btnRoomLights1Off');
const btnRoomLights1On = document.getElementById('btnRoomLights1On');
const btnRoomLights2Off = document.getElementById('btnRoomLights2Off');
const btnRoomLights2On = document.getElementById('btnRoomLights2On');
const btnScannerStop = document.getElementById('btnScannerStop');
const btnScannerFeet = document.getElementById('btnScannerFeet');
const btnScannerHead = document.getElementById('btnScannerHead');
const btnScannerFullscan = document.getElementById('btnScannerFullscan');
const btnScannerFreakout = document.getElementById('btnScannerFreakout');
const btnTiltStop = document.getElementById('btnTiltStop');
const btnTiltForward = document.getElementById('btnTiltForward');
const btnTiltBackward = document.getElementById('btnTiltBackward');
const btnTiltCenter = document.getElementById('btnTiltCenter');
const btnDoorsStop = document.getElementById('btnDoorsStop');
const btnDoorsClose = document.getElementById('btnDoorsClose');
const btnDoorsOpen = document.getElementById('btnDoorsOpen');



let timestampsArray = [];

function addBlankTimestamp() {
    addCustomTimestamp('Tilt', 0, 'Tilt down');
}

function addCustomTimestamp(option, value, comment = '') {
    const currentTime = Math.floor(video.currentTime);
    const newTimestamp = {
        time: currentTime,
        selectedOption: option,
        integerValue: value,
        comment: comment
    };
    timestampsArray.push(newTimestamp);
    updateTimestampsList();
}

function updateTimestampsList() {
    timestampsContainer.innerHTML = '';
    timestampsArray.forEach((timestamp, index) => {
        const listItem = document.createElement('div');
        listItem.classList.add('timestamp-item');

        // for time
        const timestampInput = document.createElement('input');
        timestampInput.classList.add('time');
        timestampInput.type = 'text';
        timestampInput.value = formatTime(timestamp.time);
        timestampInput.addEventListener('change', (event) => {
            const newTime = parseTime(event.target.value);
            if (!isNaN(newTime)) {
                timestampsArray[index].time = newTime;
                updateTimestampsList();
            } else {
                event.target.value = formatTime(timestamp.time);
            }
        });
        listItem.appendChild(timestampInput);

        const optionSelect = document.createElement('select');
        optionSelect.addEventListener('change', (event) => {
            timestampsArray[index].selectedOption = event.target.value;
        });
        ['Tilt', 'Doors', 'Scanner', 'Roomlights 1', 'Roomlights 2', 'LEDs'].forEach((option) => {
            const optionElement = document.createElement('option');
            optionElement.value = option;
            optionElement.text = option;
            optionSelect.appendChild(optionElement);
        });
        optionSelect.value = timestamp.selectedOption;
        listItem.appendChild(optionSelect);

        // for value
        const valueInput = document.createElement('input');
        valueInput.classList.add('value');
        valueInput.type = 'number';
        valueInput.value = timestamp.integerValue;
        valueInput.addEventListener('change', (event) => {
            const newValue = parseInt(event.target.value);
            if (!isNaN(newValue)) {
                timestampsArray[index].integerValue = newValue;
            } else {
                event.target.value = timestampsArray[index].integerValue;
            }
        });
        listItem.appendChild(valueInput);

        // for comments
        const commentInput = document.createElement('input');
        commentInput.classList.add('comment');
        commentInput.type = 'text';
        commentInput.value = timestamp.comment;
        commentInput.addEventListener('change', (event) => {
            timestampsArray[index].comment = event.target.value;
        });
        listItem.appendChild(commentInput);

        const deleteBtn = document.createElement('button');
        deleteBtn.textContent = 'Delete';
        deleteBtn.addEventListener('click', () => {
            timestampsArray.splice(index, 1);
            updateTimestampsList();
        });
        listItem.appendChild(deleteBtn);

        timestampsContainer.appendChild(listItem);
    });
}

function formatTime(seconds) {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = Math.floor(seconds % 60);
    return `${minutes.toString().padStart(2, '0')}:${remainingSeconds.toString().padStart(2, '0')}`;
}

function parseTime(timeString) {
    const timeParts = timeString.split(':');
    const minutes = parseInt(timeParts[0]);
    const seconds = parseInt(timeParts[1]);
    if (isNaN(minutes) || isNaN(seconds)) {
        return NaN;
    }
    return minutes * 60 + seconds;
}

function saveData() {
    const data = JSON.stringify(timestampsArray, null, 2);
    const blob = new Blob([data], { type: 'application/json' });

    // Check if the browser supports the 'download' attribute
    if ('download' in document.createElement('a')) {
        const url = URL.createObjectURL(blob);
        const link = document.createElement('a');
        link.href = url;
        link.download = 'timestamps.json';
        link.click();
        URL.revokeObjectURL(url);
    } else {
        alert('File saving is not supported in this browser.');
    }
}

function loadData(event) {
    const file = event.target.files[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = function (event) {
        try {
            const loadedData = JSON.parse(event.target.result);
            if (Array.isArray(loadedData)) {
                timestampsArray = loadedData;
                updateTimestampsList();
            } else {
                alert('Invalid data format. The file should contain a JSON array.');
            }
        } catch (error) {
            alert('Error loading data. Please ensure the file contains valid JSON data.');
        }
    };
    reader.readAsText(file);
}


saveBtn.addEventListener('click', saveData);
loadFileInput.addEventListener('change', loadData);
loadBtn.addEventListener('click', function () {
    loadFileInput.click();
});

// quick edits
btnAddTimestamps.addEventListener('click', addBlankTimestamp);
btnLEDsOff.addEventListener('click', function () { addCustomTimestamp('LEDs', 0, "LEDs off"); });
btnLEDsBlue.addEventListener('click', function () { addCustomTimestamp('LEDs', 1, "LEDs blue"); });
btnLEDsRed.addEventListener('click', function () { addCustomTimestamp('LEDs', 2, "LEDs red"); });
btnLEDsWhite.addEventListener('click', function () { addCustomTimestamp('LEDs', 3, "LEDs white"); });
btnLEDsCycle.addEventListener('click', function () { addCustomTimestamp('LEDs', 4, "LEDs cycle"); });
btnRoomLights1Off.addEventListener('click', function () { addCustomTimestamp('Roomlights 1', 0, "Roomlights 1 off"); });
btnRoomLights1On.addEventListener('click', function () { addCustomTimestamp('Roomlights 1', 1, "Roomlights 1 on"); });
btnRoomLights2Off.addEventListener('click', function () { addCustomTimestamp('Roomlights 2', 0, "Roomlights 2 off"); });
btnRoomLights2On.addEventListener('click', function () { addCustomTimestamp('Roomlights 2', 1, "Roomlights 2 on"); });
btnScannerStop.addEventListener('click', function () { addCustomTimestamp('Scanner', 0, "Scanner stop"); });
btnScannerFeet.addEventListener('click', function () { addCustomTimestamp('Scanner', 1, "Scanner towards feet"); });
btnScannerHead.addEventListener('click', function () { addCustomTimestamp('Scanner', 2, "Scanner towards head"); });
btnScannerFullscan.addEventListener('click', function () { addCustomTimestamp('Scanner', 3, "Scanner full scan"); });
btnScannerFreakout.addEventListener('click', function () { addCustomTimestamp('Scanner', 4, "Scanner freakout"); });
btnTiltStop.addEventListener('click', function () { addCustomTimestamp('Tilt', 0, "Tilt stop"); });
btnTiltForward.addEventListener('click', function () { addCustomTimestamp('Tilt', 1, "Tilt forward"); });
btnTiltBackward.addEventListener('click', function () { addCustomTimestamp('Tilt', 2, "Tilt backward"); });
// btnTiltCenter.addEventListener('click', function () { addCustomTimestamp('Tilt', 3, "Tilt center"); });
btnDoorsStop.addEventListener('click', function () { addCustomTimestamp('Doors', 0, "Doors stop"); });
btnDoorsClose.addEventListener('click', function () { addCustomTimestamp('Doors', 1, "Doors close"); });
btnDoorsOpen.addEventListener('click', function () { addCustomTimestamp('Doors', 2, "Doors open"); });

// Initialize the list of timestamps
updateTimestampsList();


document.addEventListener("DOMContentLoaded", function () {

    // Handle the "Load Video" button click event
    changeVidBtn.addEventListener('click', function () {
        loadFile.click(); // Trigger the file input click programmatically
    });

    // Handle the file input change event
    loadFile.addEventListener('change', function (event) {
        const file = event.target.files[0];
        if (file) {
            const objectURL = URL.createObjectURL(file);
            videoElement.src = objectURL;
            videoElement.load(); // Reload the video to apply the new source
            videoElement.play(); // Start playing the new video
        }
    });
});