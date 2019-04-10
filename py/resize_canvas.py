from PIL import Image
import math


def resize_canvas(old_image_path, new_image_path, canvas_width=None, canvas_height=None):
    """
    Resize the canvas of old_image_path.

    Store the new image in new_image_path. Center the image on the new canvas.

    Parameters
    ----------
    old_image_path : str
    new_image_path : str
    canvas_width : int
    canvas_height : int
    """
    im = Image.open(old_image_path)
    old_width, old_height = im.size
    
    if canvas_width:
        new_width = canvas_width
    else:
        new_width = old_width
        
    if canvas_height:
        new_height = canvas_height
    else:
        new_height = old_height

    # Center the image
    x1 = int(math.floor((new_width - old_width) / 2))
    y1 = int(math.floor((new_height - old_height) / 2))

    mode = im.mode
    if len(mode) == 1:  # L, 1
        new_background = 255
    if len(mode) == 3:  # RGB
        new_background = (255, 255, 255)
    if len(mode) == 4:  # RGBA, CMYK
        new_background = (255, 255, 255, 255)

    newImage = Image.new(mode, (new_width, new_height), new_background)
    newImage.paste(im, (x1, y1, x1 + old_width, y1 + old_height))
    newImage.save(new_image_path)
