# WebServer-FTP

<p>
                  Este es un proyecto de la asignatura de <b>Sistemas Operativos</b>, en el que tuvimos que diseñar e implementar un <b>Servidor FTP</b> solo usando <b>C</b> y funcionalidades del <b>kernel de linux</b>.
<p>
<h2> Autores:</h2>
<ul>
    <li>Leoanrdo Ulloa Ferrer C212 (<a href = "https://github.com/Leo00010011">Leo00010011</a>)</li>
    <li>Toni Cadahaía Poveda C212 (<a href = "https://github.com/tonycp">tonycp</a>)</li>
</ul>
                  <hr>
                  <p style="margin-left: 40px">
                  El objetivo principal de este proyecto era implementar un servidor que permita navegar por un conjunto de carpetas y descargar archivos usando un navegador. Para poder resolver este proyecto nos tuvimos que enfrentar a varios retos: el trabajo con <b>sockets</b>, el protocolo <b>http</b>, mucho trabajo con <b>punteros</b>, manejo de <b>memoria</b> y la <b>paralelización</b> de las funcionalidades. 
                  </p>
                  <p style="margin-left: 40px">
                  El funcionamiento del servidor consiste en aceptar constantemente conexiones y crear un  proceso a parte para atenderlas. Para poder terminar el servidor correctamente implementamos un <b>handler</b> de <b>SIGINT</b> y guardamos el <b>pid</b> de cada proceso creado para poder terminarlos enviando <b>SIGKILL</b> y cosecharlos, pues pueden estar ejecutando algo que demora, como la descarga de un archivo . El cliente puede hacer dos tipos de peticiones: cambiar de dirección o descargar un archivo y esta distinción se hace por un caracter en la <b>url</b>.
                  </p>
                  <p style="margin-left: 40px">
                  Una de las funcionalidades que nos pidieron fue implementar distintas formas de dar orden a los archivos que se muestran, nosotros implementamos 3: ordenar por nombre, ordenar por tamaño y ordenar por fecha de modificación. Quisimos que se pudieran agregar nuevos métodos de ordenación con facilidad, por lo que intentamos <b>desacoplar</b> esta parte del código lo más posible del resto. La mejor manera que encontramos de lograr esto fue que cada método de ordenación fuera un programa que que recibiera la dirección como parámetro y que respondiera el correspondiente respuesta http con el código <b>html</b>. Para poder facilitar esto creamos un conjunto de funcionalidades para generar el diseño de la ventana en <b>html</b> y enviarlo usando el protocolo <b>http</b>. También implementamos un <b>sort</b> que recibe un <b>delegado</b> con el criterio de comparación. De esta forma solo hay que agregar un programa con el nuevo criterio de comparación y reutilizar los métodos que brindamos para crear y enviar la página.
                  </p>
                  <p style="margin-left: 40px">
                  Se hicieron structs para representar los <b>http_request</b> y <b>http_response</b> y los métodos correspondientes para parsear el <b>request</b> y serializar el <b>response</b> para enviarlo. Para leer eficientemente el <b>request</b> se usa un <b>buffer</b> para el cual se creó un <b>struct</b> y varios métodos para leer del <b>socket</b>, como un método remplazando el <b>lseek</b> y otro para leer hasta que se encuentre con cierto caracter. Los <b>header</b> se guardan en una <b>linked list</b> de <b>pair key-values</b>. En el caso de la respuesta, el comportamiento depende de si se quiere descargar un archivo o cambiar de directorio. Para enviar un archivo primero se construye un <b>struct</b> <b>http_response</b> con el <b>request line</b> y los <b>header</b> necesarios, para después de serializarlo y enviarlo, y después se empieza a enviar el archivo con el método <b>sendfile</b>. En el caso de cambiar de directorio se utilizan los métodos de ordenación que fueron **comentados** que fueron comentados anteriormente.
                  </p>