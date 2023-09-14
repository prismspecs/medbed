const VLC = require("vlc-client");

const vlc = new VLC.Client({
    ip: "localhost",
    port: 8080,
    username: "steve_aoki", //username is optional
    password: "edm"
});

vlc.playFile("video.mp4");