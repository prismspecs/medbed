import json
import pygame
import time
import serial
from datetime import datetime

# Load timestamps from timestamps.json
def load_timestamps():
    try:
        with open('timestamps.json', 'r') as file:
            timestamps = json.load(file)
        return timestamps
    except Exception as e:
        print(f'Error loading timestamps: {str(e)}')
        return []

# Initialize Pygame and video playback
def init_pygame(video_file):
    pygame.init()
    screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
    pygame.display.set_caption('Video Player')
    video = pygame.movie.Movie(video_file)
    video_screen = pygame.Surface(video.get_size()).convert()
    return video, video_screen

# Serial port initialization (update port and baud rate)
def init_serial_port():
    try:
        ser = serial.Serial('/dev/cu.usbmodem1101', 38400)  # Update with your Arduino's serial port and baud rate
        return ser
    except Exception as e:
        print(f'Error initializing serial port: {str(e)}')
        return None

# Process timestamps based on video time
def process_timestamps(timestamps, video, ser):
    already_processed = set()
    video.set_display(video_screen)

    video.play()
    clock = pygame.time.Clock()

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        current_time = video.get_time() / 1000  # Get current video time in seconds

        for timestamp in timestamps:
            if (timestamp['time'] not in already_processed and
                    current_time >= timestamp['time']):
                arduino_data = f"{timestamp['selectedOption']},{timestamp['integerValue']}\n"
                ser.write(arduino_data.encode())
                print(f'Timestamp {timestamp["time"]}: Sent to Arduino: {arduino_data}')
                already_processed.add(timestamp['time'])

        clock.tick(50)  # Limit the frame rate to 50 FPS

    video.stop()
    pygame.quit()

def main():
    timestamps = load_timestamps()
    if not timestamps:
        print('No timestamps found.')
        return

    video_file = 'video.mp4'  # Update with your video file path
    video, video_screen = init_pygame(video_file)

    serial_port = init_serial_port()
    if not serial_port:
        return

    try:
        process_timestamps(timestamps, video, serial_port)
    except KeyboardInterrupt:
        pass

    serial_port.close()

if __name__ == "__main__":
    main()
