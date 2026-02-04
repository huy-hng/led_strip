import time
import colorsys
from rpi_ws281x import PixelStrip, Color

# ---------------- CONFIG ----------------

LED_COUNT      = 100
LED_PIN        = 18
LED_FREQ_HZ    = 800000
LED_DMA        = 10
LED_BRIGHTNESS = 255
LED_INVERT     = False
LED_CHANNEL    = 0

FPS              = 60
GAMMA            = 2.72
USE_MICRO_NOISE  = False
NOISE_RANGE      = 1      # ±1 only (keep tiny)

# ---------------- SETUP ----------------

strip = PixelStrip(
    LED_COUNT, LED_PIN,
    LED_FREQ_HZ, LED_DMA,
    LED_INVERT, LED_BRIGHTNESS,
    LED_CHANNEL
)
strip.begin()

# Global FastLED-style dither phase
dither_phase = 0

# ---------------- GAMMA TABLE ----------------

gamma_table = [
    int(pow(i / 255.0, GAMMA) * 255 + 0.5)
    for i in range(256)
]

# ---------------- UTILS ----------------

def clear(strip):
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, 0)
    strip.show()

def hsv_to_rgb(h, s, v):
    r, g, b = colorsys.hsv_to_rgb(h, s, v)
    return int(r * 255), int(g * 255), int(b * 255)

def micro_noise(pixel, channel, frame):
    if not USE_MICRO_NOISE:
        return 0
    n = (pixel * 37 + channel * 17 + frame * 13) & 0xFF
    return (n % (2 * NOISE_RANGE + 1)) - NOISE_RANGE

def dither_channel(value, frame, pixel, channel):
    """
    value: gamma-corrected 0–255
    """
    phase_bias = dither_phase - 127

    x = value * 255 + phase_bias

    # FastLED video bias (CRITICAL)
    if value != 0:
        x += 1

    # Optional micro-noise (safe now)
    x += micro_noise(pixel, channel, frame)

    return x >> 8

# ---------------- MAIN LOOP ----------------

try:
    frame = 0
    while True:
        t = time.time()

        for i in range(LED_COUNT):

            # Example source (intentionally low brightness)
            hue = ((i * 0.03) + t * 0.05) % 1.0
            r, g, b = hsv_to_rgb(hue, 1.0, 0.2)

            val = round(((i+1) / strip.numPixels()) * 100)
            r, g, b = val, val, val

            # --- GAMMA CORRECTION ---
            r_g = gamma_table[r]
            g_g = gamma_table[g]
            b_g = gamma_table[b]

            # --- HYBRID FASTLED-STYLE DITHER ---
            r_out = dither_channel(r_g, frame, i, 0)
            g_out = dither_channel(g_g, frame, i, 1)
            b_out = dither_channel(b_g, frame, i, 2)

            if i == 1 or i == 3: b_out = 0
            color = Color(
                max(0, min(255, r_out)),
                max(0, min(255, g_out)),
                max(0, min(255, b_out))
            )
            strip.setPixelColor(i, color)

        strip.show()
        # print(dither_phase)

        # Advance FastLED-style phase
        dither_phase = (dither_phase + 1) & 0xFF
        frame += 1

        time.sleep(1.0 / FPS)

except KeyboardInterrupt:
    clear(strip)
    strip.show()

