import { useTheme } from '@mui/system';
import 'rapidoc';

export default function RestUI() {
    const theme = useTheme();
    const { palette, typography  } = theme;

    return (
        <rapi-doc
            spec-url = "/api/openapi.yaml"
            server-url = "/api"
            default-api-server = "/api"
            render-style = "focused"
            
            theme = {palette.mode}
            bg-color = {palette.background.paper}
            text-color = {palette.primary.text}
            primary-color = {palette.primary.main}

            
            use-path-in-nav-bar = {false}
            nav-bg-color = {palette.secondary.background}
            nav-text-color = {palette.secondary.text}
            nav-hover-bg-color = {palette.secondary.dark}
            nav-hover-text-color = {palette.secondary.contrastText}
            nav-accent-color = {palette.primary.light}


            regular-font = {typography.fontFamily}
            mono-font = {typography.mono.fontFamily}
            font-size = "largest"

            update-route = {true}
            show-header = {false}
            allow-search = {false}
            allow-advanced-search = {false}
            allow-authentication = {false}
            allow-server-selection = {false}
            schema-description-expanded = {true}
            style = {{ height: 'max-content', width: "100%" }}
        />
    )

    //style = {{ height: '200px', width: "100%" }}
}