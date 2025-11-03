import * as React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import GameModesScreen from '../screens/GameModesScreen';
import LEDModeScreen from '../screens/LEDModeScreen';
import SoundModeScreen from '../screens/SoundModeScreen';
import DualModeScreen from '../screens/DualModeScreen';

export type GameModesStackParamList = {
    GameModesHome: undefined;
    LEDMode: undefined;
    SoundMode: undefined;
    DualMode: undefined;
}

const Stack = createNativeStackNavigator<GameModesStackParamList>();

export default function GameModesStack() {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="GameModesHome"
        component={GameModesScreen}
        options={{ headerShown: false }}
      />
      <Stack.Screen name="LEDMode" component={LEDModeScreen} options={{ title: 'LED Mode' }} />
      <Stack.Screen name="SoundMode" component={SoundModeScreen} options={{ title: 'Sound Mode' }} />
      <Stack.Screen name="DualMode" component={DualModeScreen} options={{ title: 'Dual Mode' }} />
    </Stack.Navigator>
  );
}