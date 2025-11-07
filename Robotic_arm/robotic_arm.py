import cv2
import mediapipe as mp
import numpy as np
import serial
import time

# -------- SETTINGS --------
SERIAL_PORT = 'COM13'   # <-- Replace with your Arduino COM port
BAUDRATE = 115200
MIN_ANGLE = 0          # Servo fully open
MAX_ANGLE = 180        # Servo fully closed
SMOOTHING = 0.8        # 0–1, higher = smoother movement
# --------------------------

# Connect to Arduino
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    time.sleep(2)  # Wait for Arduino
    print("Connected to Arduino on", SERIAL_PORT)
except Exception as e:
    print("Serial error:", e)
    ser = None

# MediaPipe Hand setup
mp_hands = mp.solutions.hands
mp_drawing = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)
hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=1,
    min_detection_confidence=0.6,
    min_tracking_confidence=0.6
)

smoothed_angle = None
prev_sent_angle = None

def calc_angle(landmarks, w, h):
    # Convert normalized coordinates to pixels
    def p(lm): return np.array([lm.x * w, lm.y * h])

    wrist = p(landmarks[0])
    middle_tip = p(landmarks[12])
    index_mcp = p(landmarks[5])

    hand_size = np.linalg.norm(index_mcp - wrist)
    if hand_size < 1e-6:
        hand_size = 1.0

    openness = np.linalg.norm(middle_tip - wrist) / hand_size

    # Map openness to full servo range (reversed)
    angle = int(np.interp(openness, [0.3, 2.0], [MAX_ANGLE, MIN_ANGLE]))
    return angle

print("Starting camera... Press 'q' to quit")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Camera not found!")
        break

    img_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(img_rgb)
    h, w, _ = frame.shape

    if results.multi_hand_landmarks:
        lm = results.multi_hand_landmarks[0]
        mp_drawing.draw_landmarks(frame, lm, mp_hands.HAND_CONNECTIONS)

        angle = calc_angle(lm.landmark, w, h)

        # Smooth movement
        if smoothed_angle is None:
            smoothed_angle = angle
        else:
            smoothed_angle = int(SMOOTHING * smoothed_angle + (1 - SMOOTHING) * angle)

        # Send to Arduino if changed
        if ser and (prev_sent_angle is None or abs(smoothed_angle - prev_sent_angle) > 1):
            print(f"Sending angle: {smoothed_angle}")
            ser.write((str(smoothed_angle) + '\n').encode())
            prev_sent_angle = smoothed_angle

        cv2.putText(frame, f"Angle: {smoothed_angle}", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    cv2.imshow("Hand → Servo", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
if ser:
    ser.close()
cv2.destroyAllWindows()