import React from "react";

import { KeyInput, KeyInputProps } from "@components";

import { useStyles } from "./styles";

export type KeyInputFieldProps = {
  helperText?: string;
  marginTop?: number;
};
export const KeyInputField: React.FC<KeyInputFieldProps & KeyInputProps> = ({
  helperText,
  marginTop = 20,
  ...inputProps
}) => {
  const classes = useStyles();

  return (
    <div className={classes.container} style={{ marginTop }}>
      <KeyInput {...inputProps} />
      {helperText && <div className={classes.helperText}>{helperText}</div>}
    </div>
  );
};
