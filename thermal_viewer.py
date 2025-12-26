import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Configure serial port - adjust COM port as needed
SERIAL_PORT = 'COM7'  # Change to your Arduino's port
BAUD_RATE = 115200

# Initialize the plot
fig, ax = plt.subplots()
thermal_image = np.zeros((24, 32))
# cmap options for thermal imaging:
#   'inferno'  - perceptually uniform, black to yellow (good for thermal)
#   'hot'      - black-red-yellow-white (classic thermal look)
#   'jet'      - rainbow colormap (blue to red)
#   'coolwarm' - blue to red diverging colormap
#   'plasma'   - perceptually uniform, purple to yellow
#   'magma'    - perceptually uniform, black to light pink
#   'viridis'  - perceptually uniform, purple to yellow-green
#   'turbo'    - improved rainbow colormap
#   'gray'     - grayscale
im = ax.imshow(thermal_image, cmap='inferno', interpolation='bilinear', vmin=20, vmax=40)
plt.colorbar(im, ax=ax, label='Temperature (°C)')
ax.set_title('MLX90640 Thermal Camera')

# Open serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

def parse_thermal_data(line):
    """Parse the thermal data from serial line."""
    try:
        line = line.strip()
        if line.startswith("START,") and line.endswith(",END"):
            data_str = line[6:-4]  # Remove START, and ,END
            values = [float(x) for x in data_str.split(",")]
            if len(values) == 768:
                return np.array(values).reshape((24, 32))
    except (ValueError, IndexError):
        pass
    return None

def update(frame_num):
    """Update function for animation."""
    global thermal_image
    
    try:
        line = ser.readline().decode('utf-8', errors='ignore')
        data = parse_thermal_data(line)
        if data is not None:
            thermal_image = data
            im.set_array(thermal_image)
            im.set_clim(vmin=thermal_image.min(), vmax=thermal_image.max())
    except Exception as e:
        print(f"Error: {e}")
    
    return [im]

# Wait for Arduino to be ready
print("Waiting for sensor...")
while True:
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if line == "READY":
        print("Sensor ready!")
        break
    elif line.startswith("ERROR"):
        print(f"Sensor error: {line}")
        exit(1)

# Start animation
ani = FuncAnimation(fig, update, interval=100, blit=True, cache_frame_data=False)
plt.show()

ser.close()
