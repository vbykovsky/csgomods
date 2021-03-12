import { makeStyles } from "@material-ui/styles";
import { colors } from "@utils";

export type TProps = {
  marginTop?: number;
};
export const useStyles = makeStyles({
  keyInput: {
    marginTop: (props: TProps) => props.marginTop,

    display: "flex",
    alignItems: "center",
  },
  keyInput_label: {
    marginRight: 10,
    fontSize: 14,
    fontWeight: 400,
    color: colors.black,
  },
  keyInput_input: {
    width: 50,
  },
});
