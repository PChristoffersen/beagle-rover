import React,{ useState } from "react";
import SwipeableViews from 'react-swipeable-views';
import { useTheme } from '@mui/material/styles';
import { Card, CardContent, CardHeader, Tab, Tabs, Box, Typography } from "@mui/material";
import SliderSteering from "./SliderSteering";


interface TabPanelProps {
    children?: React.ReactNode;
    dir?: string;
    index: number;
    value: number;
}
  

function TabPanel(props: TabPanelProps) {
    const { children, value, index, ...other } = props;
  
    return (
      <div
        role="tabpanel"
        hidden={value !== index}
        id={`full-width-tabpanel-${index}`}
        aria-labelledby={`full-width-tab-${index}`}
        {...other}
      >
        {value === index && (
          <Box sx={{ p: 3 }}>
            {children}
          </Box>
        )}
      </div>
    );
}

export default function SteeringCard() {
    const theme = useTheme();
    const [ currentTab, setCurrentTab ] = useState<number>(0);

    const handleTabChange = (event: React.SyntheticEvent, newValue: number) => {
        setCurrentTab(newValue);
    }
    const handleChangeIndex = (index: number) => {
        setCurrentTab(index);
    };

    return (
        <Card sx={{ minWidth: 375, minHeight: 375 }}>
            <Tabs value={currentTab} onChange={handleTabChange} variant="fullWidth">
                <Tab label="Sliders"/>
                <Tab label="Pads"/>
                <Tab label="Controller"/>
            </Tabs>
            <CardContent>
                <SwipeableViews
                    axis={theme.direction === 'rtl' ? 'x-reverse' : 'x'}
                    index={currentTab}
                    onChangeIndex={handleChangeIndex}
                >
                    <TabPanel value={currentTab} index={0}>
                        <SliderSteering />
                    </TabPanel>
                    <TabPanel value={currentTab} index={1}>
                        Pads
                    </TabPanel>
                    <TabPanel value={currentTab} index={2}>
                        Controller
                    </TabPanel>
                </SwipeableViews>
            </CardContent>
        </Card>
    )
}