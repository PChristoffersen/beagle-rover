import CssBaseline from '@mui/material/CssBaseline';
import { ThemeProvider, createTheme, StyledEngineProvider } from '@mui/material/styles';
import "@fontsource/varela-round";
import "@fontsource/source-code-pro";

interface Props {
    children: React.ReactNode;
}


const theme = createTheme({
    palette: {
        //mode: 'dark',
        primary: {
            main: "#0277bd",
            light: "#58a5f0",
            dark: "#004c8c",
        },
        secondary: {
            main: "#424242",
            light: "#6d6d6d",
            dark: "#1b1b1b",
        },
        background: {
            default: "#e1f5fe",
        }
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