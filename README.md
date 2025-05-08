<!-- Copyright 2024-2025 Kieran W Harvie. All rights reserved. -->

**:warning:Living Document: All Parts are Subject to Change:warning:**

# Tessellating Bézier Curves and Surface

<table>
<tr>
<th>
<p align="center">
	<img src="bicubic.png" alt="A picture of a Bicubic Bézier surface" width = 500px style="background:white;border-style: groove;border-radius:5px;padding:5px">
	<br>
	    A Bicubic Bézier surface whoes trianglulation was calculated in the Tesselation shader. 
		Only the 16 white points where sent to the graphics card.
	<br>
</p>
</th>
<th>
<p align="center">
	<img src="conic.png" alt="A picture of a Conic section" width = 500px style="background:white;border-style: groove;border-radius:5px;padding:5px">
	<br>
		A Conic section defined by three points and two tangents caulated as a rational quadratic Bézier curve.
		Only 3 points where sent to the graphics card.
	<br>
</p>
</p>
</th>
</tr>
<tr>
<th>
<p align="center">
	<img src="composite.png" alt="A picture composite cubic Bézier curve" width = 500px style="background:white;border-style: groove;border-radius:5px;padding:5px">
	<br>
	    A composite cubic Bézier with velocity continuity.
		The geometry is specified once and locally at each point defining the curve.
	<br>
</p>
</th>
<th>
<p align="center">
	<img src="intersections.png" alt="A picture of the intersections of a cubic Bézier curve with the x-axis." width = 500px style="background:white;border-style: groove;border-radius:5px;padding:5px">
	<br>
		The intersections of a cubic Bézier curve with the x-axis.
		The intersections are calculated CPU side and the curve is rendered using the normal method.
	<br>
</p>
</p>
</th>
</tr>
</table>
This repo demonstrates how to use OpenGL's tessellation shaders to render Bézier curves and surfaces.
These curves and surfaces are an intuitive, extendable, and numerically stable way to add character to renders.
The tessellation shaders provide a performance minded and memory efficient way to implement Bézier curves and surfaces in OpenGL,
what a great combination!


## Summaries

Looking at the structure of the tessellation shader I believe it was built for these types of operations.
As such the purpose of each file in the demonstrations follows naturally from their shader type,
which can be read about [here](https://lazyfoo.net/tutorials/SDL/index.php),
with main.c acting as minimal driving code to build the shanders and send them user input.
Hence the following table only gives a mathematical summary of each demonstration:

| Demonstration | Summary |
|--|--|
| cubic curve | Implementation of a [cubic Bézier curve](https://en.wikipedia.org/wiki/B%C3%A9zier_curve) which can be thought of as a smooth line between two end point and their tangents. |
| bicubic rectangle | Implementation of a [bicubic Bézier surface](https://en.wikipedia.org/wiki/B%C3%A9zier_surface) which can be thought of as a soft rectangle controlled by 16 points which can be used to 'pull' the rectangle towards them. |
| cubic triangle | Implementation of a [biquadratic Bézier surface](https://en.wikipedia.org/wiki/B%C3%A9zier_surface) which can be thought of as a soft triangle controlled by 10 points which can be used to 'pull' the triangle towards them. |
| conic section | Implementation of a [rational quadratic Bézier curve](https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Rational_B%C3%A9zier_curves) which can be thought of as as the [conic section](https://en.wikipedia.org/wiki/Conic_section) between two endpoints, their tangents, and an arbitrary third point. |
| width and color | Implementation of a [cubic Bézier curve](https://en.wikipedia.org/wiki/B%C3%A9zier_curve) where both width and color are also parameterized by the control points alongside position. |
| composite curve | Implementation of a [composite Bézier curve](https://en.wikipedia.org/wiki/Composite_B%C3%A9zier_curve) with velocity continuity such that the geometry is specified once and locally at each point defining the curve. |
| intersections | Implementation of the intersections of a [cubic Bézier curve](https://en.wikipedia.org/wiki/B%C3%A9zier_curve) and a line using iterative subdivision. Note that the intersection isn't directly related to OpenGl and reuses the shaders from the cubic curve demonstration. |

And more to come!

## Additional resources
- Set-up: For general SDL examples and set-up advice consider Lazy Foo' Productions [SDL Tutorials](https://lazyfoo.net/tutorials/SDL/index.php).
- Tessellation Shader: For background on the Tessellation Shader consult the OpenGL wiki's [Tessellation page](https://www.khronos.org/opengl/wiki/Tessellation).
- Mathematics: For further reading into how the presented algorithms work consider reading "[Curves and Surfaces for CAGD: A Practical Guide](https://books.google.com.au/books/about/Curves_and_Surfaces_for_CAGD.html?id=5HYTP1dIAp4C&source=kp_book_description&redir_esc=y)".

