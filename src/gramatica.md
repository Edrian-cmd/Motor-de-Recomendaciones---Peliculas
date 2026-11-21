comando    = buscar | calificar | recomendar | explicar
           | deshacer | historial | descubrir | siguiente
           | afinidad | usuario | dev | ayuda | salir ;

buscar     = "buscar" ( "id" ENTERO
                      | "genero" CADENA
                      | "anio" ENTERO [ ".." ENTERO ] ) ;
calificar  = "calificar" "id" ENTERO ENTERO ;
recomendar = "recomendar" "top" ENTERO [ "genero" CADENA ] ;
explicar   = "explicar" "id" ENTERO ;
afinidad   = "afinidad" "usuario" ENTERO ;
usuario    = "usuario" ENTERO ;          (* cambia el usuario activo *)
dev        = "dev" ( "on" | "off" ) ;
deshacer   = "deshacer" ;   historial = "historial" ;
descubrir  = "descubrir" ;  siguiente = "siguiente" ;
ayuda      = "ayuda" ;      salir     = "salir" ;
