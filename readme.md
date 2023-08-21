# Installation
npm install mpv fs serialport
brew install mpv


# To do:
Add video load button to change video

# Important Notes
Make sure that "full-screen-api.allow-trusted-requests-only" is false in Firefox about:config settings.


# Installation


# Setup and Useage


+ [New 24v motor](https://www.reichelt.de/de/de/dc-motor-buerstenlos-24-v-188-w-60-ncm-act-57blf03-p271496.html?nbc=1&trstct=lsbght_sldr::116956&&r=1)
+ [Video on how to control big motors with Arduino](https://www.youtube.com/watch?v=gpHCOny_neQ)
+ [Motor Shield R3 w Stepper tutorial](https://www.makerguides.com/arduino-motor-shield-stepper-motor-tutorial/)
+ [List of Motor Drivers](https://www.pololu.com/category/11/brushed-dc-motor-drivers)
+ [Another motor driver](https://www.omc-stepperonline.com/digital-brushless-dc-motor-driver-12v-24vdc-max-5a-150w-bld-405s)


Create a website that has a video "video.mp4" which takes up at maximum 50% of the page width. There should be a toolbar below the video with a button "Add Timestamp". There should be a JSON array "timestampArray".

Clicking "Add Timestamp" adds a timestamp to the JSON array using the video's current timestamp. The contents of the JSON array should also appear at the right side of the window. Each timestamp should be an editable text box. For each timestamp, there should also be a dropdown menu with the following options: "Motor 1", "Motor 2", "Motor 3", and "Lights". For each timestamp there should also be a textbox "Value" which accepts integer values. Selecting a value from the dropdown list, or editing the integer value, or editing the timestamp value should update the JSON array. The objects in the array should contain a timestamp, a value from the dropdown list, and an integer value.

Furthermore, each timestamp which appears at the right side of the screen should have a "Delete" buttons. The Delete button removes that timestamp object from the browser as well as the array.

In the toolbar below the video, there should be "Save" and "Load" buttons which allow the user to save and load the json array from a text file.


## Player

Create a node.js project that reads a file, timestamps.json, and plays a video, video.mp4, in full screen. The json file is full of objects that contain timecodes, looking like this:

[
  {
    "time": 0,
    "selectedOption": "Lights",
    "integerValue": 55
  },
  {
    "time": 1,
    "selectedOption": "Motor 1",
    "integerValue": 23
  }
]

When one of these timecodes is passed in the video, it should print a line in the console with the other information (selectedOption and integerValue). These should only be displayed once. In addition to displaying that information in the console, it should be sent to Arduino via Serial communication.