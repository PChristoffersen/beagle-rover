import CssBaseline from '@mui/material/CssBaseline';
import { ThemeProvider, createTheme, StyledEngineProvider } from '@mui/material/styles';
import { red } from '@mui/material/colors';

interface Props {
    children: React.ReactNode;
}


const theme = createTheme({
    palette: {
        mode: 'dark',
        primary: {
            main: '#2196f3',
        },
        secondary: {
              main: '#ffea00',
        },
        error: {
              main: red.A400,
        },
    },
});
  
export default function ThemeConfig({ children }: Props) {
    return (
        <StyledEngineProvider injectFirst>
            <ThemeProvider theme={theme}>
                <CssBaseline enableColorScheme />
                {children}
            </ThemeProvider>
        </StyledEngineProvider>
    );
}