import CssBaseline from '@mui/material/CssBaseline';
import { ThemeProvider, createTheme, StyledEngineProvider } from '@mui/material/styles';
import { red } from '@mui/material/colors';
import "@fontsource/varela-round";
import "@fontsource/source-code-pro";

interface Props {
    children: React.ReactNode;
}


const theme = createTheme({
    palette: {
        mode: 'dark',
        /*
        primary: {
            main: '#2196f3',
        },
        */
        secondary: {
              main: '#ffea00',
        },
        error: {
              main: red.A400,
        },
        /*
        background: {
            default: '#505050',
        }
        */
    },
    typography: {
        fontFamily: `"Varela Round", "Helvetica", "Arial", "sans-serif"`,

        // @ts-expect-error
        mono: {
            fontFamily: `"Source Code Pro", "Courier", monospace`,
            fontSize: 14,
            fontWeight: 400,
        }
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