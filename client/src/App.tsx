import Router from './Router';
import ThemeConfig from './theme';
import GlobalStyles from './theme/globalStyles';
import ScrollToTop from './components/ScrollToTop';
import { useEffect } from 'react';
import { useAppDispatch } from './hooks';
import { backendActions } from './features/backendSlice';
  

export default function App() {
    const dispatch = useAppDispatch();

    useEffect(() => {
        dispatch(backendActions.startConnecting())
    });

    return (
        <ThemeConfig>
            <ScrollToTop />
            <GlobalStyles />
            <Router />
        </ThemeConfig>
    );
}
