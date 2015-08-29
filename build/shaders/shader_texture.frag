/* Input */
varying vec2 v_Texcoord0;
uniform sampler2D u_Texture0;

void main()
{
    gl_FragColor = texture2D( u_Texture0, v_Texcoord0 );
}

