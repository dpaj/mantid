import numpy
from wand.image import Image
from wand.sequence import Sequence
from wand.drawing import Drawing
from wand.color import Color
import PyQt4
from PyQt4.QtCore import QByteArray, QBuffer

def animate_slice(sv, name, start, end, filename, num_frames=10, font_size=24):
    """Generate an animated gif of a 2D slice moving through a third dimension.

    Args:
        sv (SliceViewer): A sliceviewer instance.
        name (str): The name of the third dimension to use.
        start (float): The starting value of the third dimension.
        end (float): The end value of the third dimension.
        filename (str): The file to save the gif to.

    Kwargs:
        num_frames (int): The number of frames the gif should contain.
        font_size: (int): The size of the caption.
    """
    #Generate all the individual frames
    images = []
    for z in numpy.linspace(start, end, num_frames):
        sv.setSlicePoint(name, z)
        sv.refreshRebin()
        qimg = sv.getImage().toImage()
        data = QByteArray()
        buf = QBuffer(data)
        qimg.save(buf, "PNG")
        image = Image(blob=str(data))
        captionstrip_size = font_size + 10
        #To add whitespace to the top, we add a vertical border,
        #then crop out the bottom border
        image.border(Color("#fff"), 0, captionstrip_size)
        image.crop(0, 0, image.width, image.height - captionstrip_size)
        #Write the caption into the whitespace
        draw = Drawing()
        draw.font_size = font_size
        draw.text(5, font_size,"%s = %g" % (name,z))
        draw(image)
        images.append(image)
    #Create a new image with the right dimensions
    animation = Image(width=images[0].width, height=images[0].height)
    #Copy in the frames from all the generated images
    for image in images:
        animation.sequence.append(image.sequence[0])
    #Drop the initial blank frame
    del animation.sequence[0]
    #Write the animation to disk
    animation.save(filename=filename)
