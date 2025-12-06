import matplotlib.pyplot as plt
import numpy as np
import json
from pathlib import Path


def load_geometry_segments(json_path):
    """
    Load all segments from the 'Geometry' section of the JSON file.

    Returns:
        segments: list of dicts with keys:
          - 'ax', 'ay', 'bx', 'by'
          - 'type' (string, e.g. 'Mirror', 'Object', 'Target')
    """
    json_path = Path(json_path)

    with json_path.open("r") as f:
        data = json.load(f)

    geometry = data.get("Geometry", [])

    segments = []
    for geom_obj in geometry:
        seg_type = geom_obj.get("Type", "Unknown")
        for seg in geom_obj.get("Segments", []):
            ax = seg["A"]["X"]
            ay = seg["A"]["Y"]
            bx = seg["B"]["X"]
            by = seg["B"]["Y"]

            segments.append({
                "ax": ax,
                "ay": ay,
                "bx": bx,
                "by": by,
                "type": seg_type,
            })

    return segments

def plot_segments(segments, title="Geometry"):
    """
    Plot all segments as lines.
    Different 'Type' values get different colors.
    """
    fig, ax = plt.subplots(figsize=(16, 10))

    # Assign a color to each geometry type
    color_cycle = plt.rcParams["axes.prop_cycle"].by_key()["color"]
    type_to_color = {}
    next_color_index = 0

    for seg in segments:
        seg_type = seg["type"]
        if seg_type not in type_to_color:
            type_to_color[seg_type] = color_cycle[next_color_index % len(color_cycle)]
            next_color_index += 1

        color = type_to_color[seg_type]

        ax.plot(
            [seg["ax"], seg["bx"]],
            [seg["ay"], seg["by"]],
            "-",  # solid line
            label=seg_type if seg_type not in ax.get_legend_handles_labels()[1] else "",
            color=color,
        )

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_title(title)
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True)

    # Only show legend if there is more than one type
    if len(type_to_color) > 0:
        ax.legend()

    plt.show()



filePath = "D:\\University Programming\\NE-451-Final-Project-Raytracing\\Moth-Eye-Raytracing\\x64\\Debug\\Simulations\\Layers_80\\Perturb_AVG_0.json"

segments = load_geometry_segments(filePath)

plot_segments(segments, title=f"Geometry from {filePath}")