import { forwardRef } from 'react';
import { Box } from '@mui/material';

// ----------------------------------------------------------------------

const Page = forwardRef(({ children, ...other }: any, ref) => (
  <Box ref={ref} {...other}>
    {children}
  </Box>
));

//export default function Page({ children }: Props)
export default Page;
