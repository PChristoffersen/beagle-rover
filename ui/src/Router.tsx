import { useRoutes } from 'react-router-dom';

import MainLayout from './layouts/main';

import Home from './pages/Home';
import Controls from './pages/Controls';
import Motors from './pages/Motors';
import LEDS from './pages/LEDS';
import Telemetry from './pages/Telemetry';
import FullscreenLayout from './layouts/fullscreen';
import Test from './pages/Test';
import Swagger from './pages/Swagger';


interface PageDefinition {
    id: string,
    name: string,
    to: string,
}

export const pages = [
    { id: "main",       name: 'Main',       to: '/' },
    { id: "controls",    name: 'Controls',  to: '/controls' },
    { id: "telemetry",  name: 'Telemetry',  to: '/telemetry' },
    { id: "leds",       name: 'LEDS',       to: '/leds' },
    { id: "motors",     name: 'Motors',     to: '/motors' },
] as PageDefinition[];


export default function Router() {
    return (
        useRoutes([
            { 
                path: '/', 
                element: <MainLayout />,
                
                children: [
                    { path: 'controls', element: <Controls /> },
                    { path: 'leds', element: <LEDS /> },
                    { path: 'telemetry', element: <Telemetry /> },
                    { path: 'motors', element: <Motors /> },
                    { element: <Home />, index: true },
                ]
            },
            { 
                path: '/fullscreen', 
                element: <FullscreenLayout />,
                
                children: [
                    { path: 'swagger', element: <Swagger /> },
                    { element: <Test />, index: true },
                ]
            },
        ])
    );
}