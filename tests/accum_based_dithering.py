import time
import random
import colorsys
from rpi_ws281x import PixelStrip, Color

# ---------------- CONFIG ----------------

LED_COUNT      = 16
LED_PIN        = 18
LED_FREQ_HZ    = 800000
LED_DMA        = 10
LED_BRIGHTNESS = 255
LED_INVERT     = False
LED_CHANNEL    = 0

FPS             = 60
NOISE_AMOUNT    = 1     # 1–4 recommended per channel
BASE_BRIGHTNESS = 20      # Low brightness target (5–40 is typical)

# ---------------- SETUP ----------------

strip = PixelStrip(
    LED_COUNT, LED_PIN,
    LED_FREQ_HZ, LED_DMA,
    LED_INVERT, LED_BRIGHTNESS,
    LED_CHANNEL
)
strip.begin()

brightness_accum = [random.randint(0, 255) for _ in range(LED_COUNT)]

# ---------------- UTILS ----------------

def hsv_to_rgb(h, s, v):
    r, g, b = colorsys.hsv_to_rgb(h, s, v)
    return int(r * 255), int(g * 255), int(b * 255)

def clamp8(x):
    return max(0, min(255, x))


def scale_video(value, scale):
    """FastLED-style video scaling (prevents black crush)."""
    return (value * scale + 255) >> 8


def clear(strip):
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, 0)
    strip.show()


# ---------------- MAIN LOOP ----------------

try:
    while True:
        t = time.time()

        for i in range(LED_COUNT):

            # Example color source
            hue = ((i * 0.03) + t * 0.06) % 1.0
            r, g, b = hsv_to_rgb(hue, 1.0, 0.05)  # deliberately low value

            val = round(((i+1) / LED_COUNT) * 100)
            r, g, b = val, val, val

            # --- PER-CHANNEL TEMPORAL DITHERING ---
            # Target brightness in 8.8 fixed point
            target = BASE_BRIGHTNESS << 8

            # Add subtle random noise
            noise = random.randint(-NOISE_AMOUNT, NOISE_AMOUNT)
            # noise = ((i * 37 + int(t * 1000)) & 7) - 4

            target += noise << 8

            # Accumulate brightness
            brightness_accum[i] += target

            # Extract integer brightness
            dithered = brightness_accum[i] >> 8
            brightness_accum[i] &= 0xFF

            # Clamp safely
            dithered = max(0, min(255, dithered))

            # Apply FastLED-style video scaling
            r2 = scale_video(r, dithered)
            g2 = scale_video(g, dithered)
            b2 = scale_video(b, dithered)


            if i == 1 or i == 3: b2 = 0
            strip.setPixelColor(i, Color(
                clamp8(r2),
                clamp8(g2),
                clamp8(b2)
            ))

        strip.show()
        time.sleep(1.0 / FPS)

except KeyboardInterrupt:
    clear(strip)
    strip.show()
