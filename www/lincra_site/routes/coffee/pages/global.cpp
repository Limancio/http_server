#include "http_server/http_server.h"
#include "../code/usuario.h"

extern "C"
{
    HTML_FUNCTION(mostrar_usuario) {
        string guest_id;
	    bool success = get_guest_cookie(&guest_id, server_data, string_list, request_data);

        if(success) {
            user user;
            get_user_by_guest_id(&user, guest_id, server_data, string_list);
            string_free(string_list, guest_id);

            INNER_HTML(
                <a class="navegacion__enlace navegacion__enlace__usuario" id="navegacion__usuario" href="#">{user.username}
    				<ul class="navegacion__opciones navegacion__enlace__usuario">
                        <li>
                            <a href="{URL_PATH}/entry">Crear Entrada</a>
                        </li>
                        <li>
                            <a href="{URL_PATH}/profile">Perfil</a>
                        </li>
                        <li>
                            <a href="{URL_PATH}/index/logout">Cerrar Sesion</a>
                        </li>
    				</ul>
                </a>
            );
            
            REPLACE_HTML(user.username);
            
            // @temporary:
            string_free(string_list, user.id);
            string_free(string_list, user.username);
        } else {
            INNER_HTML(
                <a class="navegacion__enlace" href="#" id="button" onclick="mostrar_login()">Login</a>
                <a class="navegacion__enlace" href="#" id="button" onclick="mostrar_registro()">Register</a>
            );
        }
    }
}