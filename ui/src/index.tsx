import * as React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux'
import { BrowserRouter } from 'react-router-dom'
import "@fontsource/roboto/latin-400.css";
import "@fontsource/roboto/latin-500.css";

import App from './App';
import { setupStore } from './store'


const store = setupStore()


ReactDOM.render(
    <Provider store={store}>
        <BrowserRouter>
            <App />
        </BrowserRouter>
    </Provider>,
    document.querySelector('#root'),
);

