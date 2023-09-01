// all values run 0-100
// motors 0-100
// lights 0-100 off -> full brightness

const video = document.getElementById('video');
const timestampsContainer = document.getElementById('timestampsContainer');
const addTimestampBtn = document.getElementById('addTimestampBtn');
const saveBtn = document.getElementById('saveBtn');
const loadFileInput = document.getElementById('loadFile');
const loadBtn = document.getElementById('loadBtn');

let timestampsArray = [];

function addTimestamp() {
    const currentTime = Math.floor(video.currentTime);
    const newTimestamp = {
        time: currentTime,
        selectedOption: 'Motor 1',
        integerValue: 0,
    };
    timestampsArray.push(newTimestamp);
    updateTimestampsList();
}

function updateTimestampsList() {
    timestampsContainer.innerHTML = '';
    timestampsArray.forEach((timestamp, index) => {
        const listItem = document.createElement('div');
        listItem.classList.add('timestamp-item');

        const timestampInput = document.createElement('input');
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
        ['Kipp/Tilt', 'Doors', 'Scanner 1', 'Lights 1', 'Lights 2', 'Lights 3', 'Lights 4', 'Lights 5 (space)'].forEach((option) => {
            const optionElement = document.createElement('option');
            optionElement.value = option;
            optionElement.text = option;
            optionSelect.appendChild(optionElement);
        });
        optionSelect.value = timestamp.selectedOption;
        listItem.appendChild(optionSelect);

        const valueInput = document.createElement('input');
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

addTimestampBtn.addEventListener('click', addTimestamp);
saveBtn.addEventListener('click', saveData);
loadFileInput.addEventListener('change', loadData);
loadBtn.addEventListener('click', function () {
    loadFileInput.click();
});

// Initialize the list of timestamps
updateTimestampsList();


document.addEventListener("DOMContentLoaded", function () {

    // Handle the "Load Video" button click event
    document.getElementById('changeVidBtn').addEventListener('click', function () {
        const file = event.target.files[0];
        if (file) {
            const objectURL = URL.createObjectURL(file);
            videoElement.src = objectURL;
            videoElement.load(); // Reload the video to apply the new source
            videoElement.play(); // Start playing the new video
        }
    });
});