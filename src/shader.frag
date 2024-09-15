#version 330 core
out vec4 FragColor;
in vec2 texCoord;

uniform vec2 u_min;
uniform vec2 u_max;
uniform int u_max_iterations;

bool isMandelbrot(vec2 c, int max_iterations)
{
    vec2 z = vec2(0.0, 0.0);
    for (int i = 0; i < max_iterations; ++i)
    {
        z = vec2(z.x * z.x - z.y * z.y + c.x, 2.0 * z.x * z.y + c.y);
        if (dot(z, z) > 4.0)
            return false;
    }
    return true;
}

void main()
{
    vec2 c = mix(u_min, u_max, texCoord);
    if (isMandelbrot(c, u_max_iterations))
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Inside Mandelbrot set, black
    else
        FragColor = vec4(texCoord.x, texCoord.y, 1.0 - texCoord.x, 1.0); // Gradient color
}
