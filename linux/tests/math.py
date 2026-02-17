def rgbw_to_24bit(r=0, g=0, b=0, w=0):
    return (w << 24) | (r << 16) | (g << 8) | b

