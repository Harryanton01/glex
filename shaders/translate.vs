#version 130

in vec3 position;

uniform mat4 view;

uniform mat4 model;

uniform vec3 colour;

out vec3 frag_color;

mat4 projection(
    float angle_of_view_y,
    float aspect_ratio,
    float z_near,
    float z_far
    ) {
    // from https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
    return mat4(
             vec4((1.0/(tan(angle_of_view_y/2.0))) * aspect_ratio, 0.0, 0.0, 0.0),
             vec4(0.0, angle_of_view_y, 0.0, 0.0),
             vec4(0.0, 0.0, (z_far+z_near)/(z_near-z_far), -1.0),
             vec4(0.0, 0.0, (2.0*z_far*z_near)/(z_near-z_far), 0.0)
           );
}

void main() {
      gl_Position = projection(radians(90.0), 4/3, 1.0, 100.0)
                    * view
		      	    * model
                    * vec4(position, 1.0f);
      frag_color = colour;
}
