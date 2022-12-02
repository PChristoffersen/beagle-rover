//import { useTheme } from '@mui/material/styles';
import { GlobalStyles as GlobalThemeStyles } from '@mui/material';

// ----------------------------------------------------------------------

export default function GlobalStyles() {
    //const theme = useTheme();

    return (
        <GlobalThemeStyles
            styles={{
                ul: { 
                    margin: 0, padding: 0, listStyle: 'none' 
                }
           }}
        />
    );
}