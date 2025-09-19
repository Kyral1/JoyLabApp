import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import AppNavigator from './navigation/AppNavigator';

export default function JoyLab() {
  return (
    <NavigationContainer>
      <AppNavigator />
    </NavigationContainer>
  );
}