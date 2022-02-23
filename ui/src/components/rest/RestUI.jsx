import { useTheme } from '@emotion/react';
import 'rapidoc';

export default function RestUI() {
    const theme = useTheme();
    const { palette, typography  } = theme;

    return (
        <rapi-doc
            spec-url = "/api/openapi.yaml"
            server-url = "/api"
            render-style = "focused"
            
            theme = {palette.mode}
            bg-color = {palette.background.paper}
            text-color = {palette.text.secondary}
            primary-color = {palette.primary.main}
            regular-font = {typography.fontFamily}
            nav-text-color = {palette.primary.main}
            nav-hover-bg-color = {palette.action.hover}
            nav-hover-text-color = {palette.text.primary}
            nav-accent-color = {palette.action.active}
            font-size = "largest"

            update-route = {true}
            show-header = {false}
            allow-search = {false}
            allow-advanced-search = {false}
            allow-authentication = {false}
            allow-server-selection = {false}
            style = {{ height: 'max-content', width: "100%" }}
        />
    )

    //style = {{ height: '200px', width: "100%" }}
}