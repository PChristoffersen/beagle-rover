import { useRoutes } from 'react-router-dom';

import MainLayout from './layouts/main';

import Home from './pages/Home';
import Controls from './pages/Controls';
import Motors from './pages/Motors';
import LEDS from './pages/LEDS';
import Telemetry from './pages/Telemetry';
import FullscreenLayout from './layouts/fullscreen';
import Test from './pages/Test';
import RestAPI from './pages/RestAPI';
import FullscreenRestAPI from './pages/FullscreenRestAPI';


interface PageDefinition {
    id: string,
    name: string,
    to: string,
}

export const pages = [
    { id: "main",       name: 'Main',       to: '/' },
    { id: "controls",   name: 'Controls',  to: '/controls' },
    { id: "telemetry",  name: 'Telemetry',  to: '/telemetry' },
    { id: "leds",       name: 'LEDS',       to: '/leds' },
    { id: "motors",     name: 'Motors',     to: '/motors' },
    { id: "rest",       name: 'API',     to: '/rest' },
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
                    { path: 'rest', element: <RestAPI /> },
                    { path: 'test', element: <Test /> },
                    { element: <Home />, index: true },
                ]
            },
            { 
                path: '/', 
                element: <FullscreenLayout />,
                
                children: [
                    { path: 'rest-full', element: <FullscreenRestAPI /> },
                    { path: 'fullscreen', element: <Test /> },
                ]
            },
        ])
    );
}