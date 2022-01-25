import { useRoutes } from 'react-router-dom';

import MainLayout from './layouts/main';

import Home from './pages/Home';
import Motors from './pages/Motors';
import LEDS from './pages/LEDS';
import Telemetry from './pages/Telemetry';
import Settings from './pages/Settings';

export default function Router() {
    return (
        useRoutes([
            { 
                path: '/', 
                element: <MainLayout />,
                
                children: [
                    { path: 'motors', element: <Motors /> },
                    { path: 'leds', element: <LEDS /> },
                    { path: 'telemetry', element: <Telemetry /> },
                    { path: 'settings', element: <Settings /> },
                    { element: <Home />, index: true },
                ]
            },
        ])
    );
}