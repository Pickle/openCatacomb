/* Input */
uniform mat4 u_MVPMatrix;
attribute vec3 a_Position;
attribute vec2 a_Texcoord0;
/* Output */
varying vec2 v_Texcoord0;

void main()
{
    v_Texcoord0 = a_Texcoord0;
    gl_Position = u_MVPMatrix * vec4(a_Position, 1.0);
}
