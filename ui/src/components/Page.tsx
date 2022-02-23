import { forwardRef } from 'react';
import { Box } from '@mui/material';
import { styled } from '@mui/material/styles';

// ----------------------------------------------------------------------


const Page = forwardRef(({ children, ...other }: any, ref) => (
  <Box id="Page" ref={ref} {...other}>
      {children}
  </Box>
));


export default Page;
