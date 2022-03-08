// The function initilize the WebGL canvas
function initWebGL(){
    var canvas = document.getElementById("webgl-canvas");
    gl = canvas.getContext("webgl2");

    const ext = gl.getExtension("EXT_color_buffer_float");
    if (!ext) {
      alert("need EXT_color_buffer_float");
      return;
    }

    //keep the size of the canvas for leter rendering
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    //check for errors
    if(gl){
        console.log("WebGL succesfully initialized.");
    }else{
        console.log("Failed to initialize WebGL.")
    }
}


// This function compiles a shader
function compileShader(shader, source, type, name = ""){
    // link the source of the shader to the shader object
    gl.shaderSource(shader,source);
    // compile the shader
    gl.compileShader(shader);
    // check for success and errors
    let success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
    if(success){
        console.log(name + " shader compiled succesfully.");
    }else{
        console.log(name + " vertex shader error.")
        console.log(gl.getShaderInfoLog(shader));
    }
}

// This function links the GLSL program by combining different shaders
function linkProgram(program,vertShader,fragShader){
    // attach vertex shader to the program
    gl.attachShader(program,vertShader);
    // attach fragment shader to the program
    gl.attachShader(program,fragShader);
    // link the program
    gl.linkProgram(program);
    // check for success and errors
    if (gl.getProgramParameter(program, gl.LINK_STATUS)) {
        console.log("The shaders are initialized.");
    }else{
        console.log("Could not initialize shaders.");
    }
}

function redenerer_settings(){
    console.log("Renderer settings:");
    console.log(document.getElementById("camera_azimuthal_angle").value);
    console.log(document.getElementById("camera_polar_angle").value);
    console.log(document.getElementById("camera_distance").value);
    console.log(document.getElementById("camera_fov").value);
    console.log(document.getElementById("light_azimuthal_angle").value);
    console.log(document.getElementById("light_polar_angle").value);
}
