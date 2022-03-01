import 'rapidoc';

export default function RestUIStock() {
    return (
        <rapi-doc
            spec-url = "/api/openapi.yaml"
            server-url = "/api"
            render-style = "focused"
            
            allow-authentication = {false}
            allow-server-selection = {false}
            style = {{ height: '100%', width: "100%" }}
        />
    )
}