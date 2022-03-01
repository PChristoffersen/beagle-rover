import React,{ useState } from "react";
import { Card, CardContent, Tab, Tabs, Box } from "@mui/material";
import SliderSteering from "./SliderSteering";
import JoystickSteering from "./JoystickSteering";
import GamepadSteering from "./GamepadSteering";


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
    const [ currentTab, setCurrentTab ] = useState<number>(0);

    const handleTabChange = (event: React.SyntheticEvent, newValue: number) => {
        setCurrentTab(newValue);
    }

    return (
        <Card sx={{ minWidth: 375, minHeight: 375 }}>
            <Tabs value={currentTab} onChange={handleTabChange} variant="fullWidth">
                <Tab label="On-Screen"/>
                <Tab label="Sliders"/>
                <Tab label="Gamepad" sx={{ display: { xs: 'none', md: 'flex' }}} />
            </Tabs>
            <CardContent>
                <TabPanel value={currentTab} index={0}>
                    <JoystickSteering />
                </TabPanel>
                <TabPanel value={currentTab} index={1}>
                    <SliderSteering />
                </TabPanel>
                <TabPanel value={currentTab} index={2}>
                    <GamepadSteering />
                </TabPanel>
            </CardContent>
        </Card>
    )
}