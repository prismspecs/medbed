# Important Notes
Make sure that "full-screen-api.allow-trusted-requests-only" is false in Firefox about:config settings.



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