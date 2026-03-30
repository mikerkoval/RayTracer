#!/usr/bin/env python3
import json, subprocess, copy, os, math

os.makedirs("output/fresnel_anim", exist_ok=True)

with open("scenes/fresnel_test.json") as f:
    base = json.load(f)

total_frames = 60

for frame in range(total_frames):
    t = frame / (total_frames - 1)  # 0.0 to 1.0

    # Sweep camera from low grazing angle (t=0) to higher view (t=1)
    cam_y   = 1.6 + t * 4.0   # 1.6 -> 5.6
    look_y  = 1.5 + t * 0.5   # 1.5 -> 2.0

    s = copy.deepcopy(base)
    s['camera']['position'] = [0, cam_y, -8]
    s['camera']['look_at']  = [0, look_y, 3]
    s['camera']['aa'] = 1  # fast for animation

    with open("/tmp/fresnel_frame.json", "w") as f:
        json.dump(s, f)

    outfile = f"output/fresnel_anim/frame{frame:04d}.png"
    print(f"Frame {frame}/{total_frames}...")
    subprocess.run(["./build/render", "/tmp/fresnel_frame.json", outfile])

# Stitch to video with ffmpeg
print("Encoding video...")
subprocess.run([
    "ffmpeg", "-y", "-framerate", "24",
    "-i", "output/fresnel_anim/frame%04d.png",
    "-c:v", "libx264", "-pix_fmt", "yuv420p",
    "-crf", "18",
    "output/fresnel_animation.mp4"
])
print("Done: output/fresnel_animation.mp4")
